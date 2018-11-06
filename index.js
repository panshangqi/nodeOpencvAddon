var opencvImage = require('./node_addon/build/Release/opencv_image');
var ImagePro = {
    hello: function (_str, fn) {
        opencvImage.hello(_str, function(data) {
            console.log(data);
            if(typeof fn === 'function'){
                fn(data)
            }
        });
    }
}

module.exports = ImagePro;

opencvImage.getImageData('c:\\pict.js',120);
