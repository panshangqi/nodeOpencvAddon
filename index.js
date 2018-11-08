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

    await Sleep(5000);
    for(var i=0;i<1000;i++){

        var st = new Date().getTime()

        opencvImage.getImageData({file: 'e:\\Html\\test3.jpg',width: 1000}, function (data) {
            var et = new Date().getTime()
            console.log(`time: ${et-st}`)
            //console.log(data.data)
            //console.log(JSON.parse(data.data))

        })

        var et = new Date().getTime()
        console.log(`time: ${et-st}`)


        await Sleep(1000);
    }
    await Sleep(100000);
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


