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

async function run(){

    for(var i=0;i<1000;i++){

        var st = new Date().getTime()
        /*
        var result = opencvImage.getImageData('D:\\pps.jpg',120,function (data) {
            console.log(data)
        });
        */
        opencvImage.delay(12, function (data) {
            console.log(data)
        })
        var et = new Date().getTime()
        console.log(`time: ${et-st}`)


        await Sleep(500);
    }
}
run()
function Sleep(time){
    return new Promise(function (resolve, reject) {
        setTimeout(function () {
            resolve(time)
        }, time)
    })
}

/*

var st = new Date().getTime()
var result = opencvImage.getImageData('D:\\pps.jpg',120,function (data) {
    console.log(data)
});
var et = new Date().getTime()
console.log(`time: ${et-st}`)
console.log(result)

*/



