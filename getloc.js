'use strict';

const fs = require('fs');
const https = require('https');
const async = require('async');
const csvSync = require('csv-parse/lib/sync');

const YOUR_API_KEY = ""; //GeoLocation APIキー

const file = './bssids.csv';
let data = fs.readFileSync(file);
let res = csvSync(data, {relax_column_count: true});

//読み込んだCSVダンプ
//console.log(res);

let bssids = [];
res.forEach(function(d)
{
    let tmp = new Object();
    //BSSIDを先頭3つに絞ってリスト作成
    //CSVの0番目の列はタイムスタンプ(起動時から)
    if(d.length >= 4)
    {
        tmp.timestamp = d[0];
        tmp.wifiAccessPoints = []
        tmp.wifiAccessPoints[0] = new Object();
        tmp.wifiAccessPoints[0].macAddress = d[1].substring(0, 17);
        tmp.wifiAccessPoints[1] = new Object();
        tmp.wifiAccessPoints[1].macAddress = d[2].substring(0, 17);;
        tmp.wifiAccessPoints[2] = new Object();
        tmp.wifiAccessPoints[2].macAddress = d[3].substring(0, 17);;
        bssids.push(tmp);
    }
});

//作ったBSSIDリストダンプ
//console.log(JSON.stringify(bssids));

//GeoLocation APIへリクエスト
function post(data)
{
    return new Promise(function(resolve, reject)
    {
        let postData = 
        {
            "homeMobileCountryCode": 310,
            "homeMobileNetworkCode": 410,
            "radioType": "lte",
            "considerIp": false,
            "carrier": "Docomo",
            "wifiAccessPoints": data
        };
        console.log(JSON.stringify(postData));
        const options = 
        {
            hostname: 'www.googleapis.com',
            port: 443,
            path: '/geolocation/v1/geolocate?key=' + YOUR_API_KEY,
            method: 'POST',
            headers: 
            {
                "content-type": "application/json"
            },
        };
        
        const req = https.request(options, (res) => 
        {
            let body = "";
            console.log('statusCode:', res.statusCode);            
            res.on('data', (d) => 
            {
                body += d;
            });
        
            res.on('end', (res) => 
            {
                resolve(body);
                req.end();    
            });
        });
        
        req.on('error', (e) => 
        {
            reject(e);
            console.error(e);
        });
        
        //リクエストボディ送信
        req.write(JSON.stringify(postData));
        req.end();    
    });
}

//BSSIDリストを順次APIに投げていく
async.eachSeries(bssids, function(data, next) 
{        
    post(data.wifiAccessPoints).then(function(res)
    {
        console.log(res);
        try
        {
            let json = JSON.parse(res);
            let tmp = data.timestamp + ",";
            tmp += json.location.lat + ",";
            tmp += json.location.lng + ",";
            tmp += json.accuracy + "\r\n";
            //結果ファイル出力
            fs.appendFileSync("./result.csv", tmp);
        }
        catch(err)
        {
            console.log("get location failed.　(" + JSON.stringify(data) + ")");
        }
        next();
    });
});