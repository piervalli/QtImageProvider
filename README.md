# QtImageProvider
QtImageProvider is custom provider to read images from service public or with Bearer authentication in the url.
Images are processed asynchronous.

```
Image {
    id:img
    width: parent.width
    height: parent.height
    cache: true
    fillMode: Image.PreserveAspectFit 
    source:"image://async/http://127.0.0.1:8090/api/v003/product/image/019244cf-c72f-7a0c-8906-0a000d020f08?Authorization=Bearer%201234"
}

```

TODO List
1. clear cache
2. Add a flag to manage http status cose 304 (not modified)

