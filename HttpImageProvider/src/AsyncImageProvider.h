#ifndef ASYNCIMAGEPROVIDER_H
#define ASYNCIMAGEPROVIDER_H
#include <qqmlextensionplugin.h>

#include <qqmlengine.h>
#include <qquickimageprovider.h>
#include <QDebug>
#include <QImage>
#include <QThreadPool>
#include "httpimageprovider_global.h"
#include "qeventloop.h"
#include <QNetworkAccessManager>
#include <QFile>
#include <QNetworkReply>
#include <QDir>
#include <QSettings>
class HTTPIMAGEPROVIDER_EXPORT AsyncImageResponseRunnable : public QObject
{
    Q_OBJECT

signals:
    void done(QImage image);

public:
    AsyncImageResponseRunnable(const QString &id, const QSize &requestedSize)
        : m_id(id), m_requestedSize(requestedSize)
    {
        QDir dir;
        dir.mkpath("www/cache");
    }

    void run()
    {
        QPointer<AsyncImageResponseRunnable> instance=this;

        if(!m_id.startsWith(QStringLiteral("http")))
        {
            //it is not a url
            auto image = QImage(0, 0, QImage::Format_RGB32);
            finalize(image);
            return;
        }
        manager = QSharedPointer<QNetworkAccessManager>(new QNetworkAccessManager(this));

        m_hash=calculateMD5(m_id);
        QSettings settings(QStringLiteral("www/cache/_%1").arg(m_hash), QSettings::IniFormat);
        settings.sync();
        const auto lastModified = settings.value("Last-modified").toString();
        const auto filename = settings.value("filename").toString();

        if(!lastModified.isEmpty())
        {
            QFileInfo fileInfo(QStringLiteral("www/cache/%1").arg(filename));
            if(fileInfo.exists()) {
                QFile file(fileInfo.absoluteFilePath());
                if (file.open(QIODevice::ReadOnly))
                {
                    const auto image = QImage::fromData(file.readAll());
                    file.close();
                    finalize(image);
                    return;
                }
            }
        }

        connect(manager.data(), &QNetworkAccessManager::finished,this, &AsyncImageResponseRunnable::onFinished);
        connect(manager.data(), &QNetworkAccessManager::sslErrors,this, [instance](QNetworkReply *reply, const QList<QSslError> &errors){
            reply->deleteLater();
            qCritical() << errors;
        });
        // Start the download
        QUrl imageUrl(m_id);
        qDebug()<<"imageUrl"<<imageUrl;
        QNetworkRequest request(imageUrl);
        //request.setRawHeader("If-Modified-Since",lastModified.toLocal8Bit());

        auto reply= manager->get(request);
        connect(reply, &QNetworkReply::errorOccurred, this, [](QNetworkReply::NetworkError error){
            qCritical() << error;
        });
        m_loop.exec();
    }
public slots:


private slots:
    void onFinished(QNetworkReply* reply)
    {
        auto image = QImage(0, 0, QImage::Format_RGB32);
        do
        {
            QSettings settings(QStringLiteral("www/cache/_%1").arg(m_hash), QSettings::IniFormat);
            settings.sync();
            qCritical() << "finished"<<reply;
            QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int httpStatusCode=0;
            if (!statusCode.isValid())
            {
                qDebug() << "No status code available.";
                break;
            }

            httpStatusCode = statusCode.toInt();
            qDebug() << "HTTP Status Code:" << httpStatusCode;

            if (reply->error() != QNetworkReply::NoError)
            {
                // Error handling
                qCritical() << "Download failed:" << reply->errorString();
                break;
            }
            // Success - Read the data from the reply
            QByteArray imageData = reply->readAll();
            image = QImage();
            image.loadFromData(imageData);

            QString lastModified;
            if(reply->hasRawHeader("Last-modified"))
            {
                lastModified = reply->rawHeader("Last-modified");
            }
            // Save the image data to a file
            auto fileName =QStringLiteral("_%1.byte").arg(m_hash);
            QFile file(QStringLiteral("www/cache/%1").arg(fileName));
            if (file.open(QIODevice::WriteOnly))
            {
                file.write(imageData);
                file.close();
                qDebug() << "Image saved to:" << fileName;
                settings.setValue("Last-modified", lastModified);
                settings.setValue("filename", QStringLiteral("_%1.byte").arg(m_hash));
            } else {
                qCritical() << "Failed to save image to file.";
                lastModified=QString{};
            }
            settings.sync();

        }while(false);

        finalize(image);
        reply->deleteLater();

    }
    void finalize(const QImage &img)
    {

        if (m_requestedSize.isValid())
            emit done(img.scaled(m_requestedSize));
        else
            emit done(img);
        qCritical()<<"image resized"<<img.width() << img.height() << "from" << m_requestedSize.width()<< m_requestedSize;
        m_loop.exit();
    }
    QString calculateMD5(const QString& input) {
        // Create an MD5 hash object and compute the hash of the input
        QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Md5);
        // Convert the hash to a hexadecimal string
        return QString(hash.toHex());
    }
private:
    QSharedPointer<QNetworkAccessManager> manager;
    QString m_id;
    QString m_hash;
    QSize m_requestedSize;
    QEventLoop m_loop;
};

class HTTPIMAGEPROVIDER_EXPORT AsyncImageResponse : public QQuickImageResponse
{
public:
    /*!
     * \brief AsyncImageResponse
     * \param id is url of image
     * \param requestedSize
     * \param pool
     */
    AsyncImageResponse(const QString &id, const QSize &requestedSize, QThreadPool *pool)
    {
        qDebug()<<"image requested size "<< id << requestedSize;
        QPointer<AsyncImageResponse> instance = this;
        pool->start([instance,id, requestedSize](){
            QSharedPointer<AsyncImageResponseRunnable> runnable(new AsyncImageResponseRunnable(id, requestedSize));
            QObject::connect(runnable.data(), &AsyncImageResponseRunnable::done, instance, &AsyncImageResponse::handleDone);
            runnable->run();
        });
    }

    void handleDone(QImage image) {
        m_image = image;
        emit finished();
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }

    QImage m_image;
};
class HTTPIMAGEPROVIDER_EXPORT AsyncImageProvider : public QQuickAsyncImageProvider
{
public:
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override
    {
        AsyncImageResponse *response = new AsyncImageResponse(id, requestedSize, &pool);
        return response;
    }
    static void initializeEngine(QQmlEngine *engine){
        engine->addImageProvider("async", new AsyncImageProvider);
    }

private:
    QThreadPool pool;
};

#endif // ASYNCIMAGEPROVIDER_H
