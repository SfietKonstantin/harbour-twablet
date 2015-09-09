var express = require('express');
var app = express();

var id = 1;

app.get('/statuses/home_timeline.json', function (req, res) {
    var date = new Date();
    var ts = String(Math.round(date.getTime() / 1000) + date.getTimezoneOffset() * 60);

    var tweets = new Array;
    for (var i = 0; i < 50; i++) {
        var tweet = new Object;
        tweet.id_str = (id + i).toString();
        tweet.text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";
        tweet.favorite_count = 0;
        tweet.favorited = false;
        tweet.retweet_count = 0;
        tweet.retweeted = false;
        tweet.in_reply_to_status_id = "0";
        tweet.source = "Test app";
        tweet.created_at = ts;
        tweet.user = new Object;
        tweet.user.id_str = "0";
        tweet.user.id_str = i.toString();
        tweet.user.name = "User " + i;
        tweet.user.screen_name = "User_" + i;
        tweets.push(tweet);
    }
    id = id + 50
    
    res.setHeader('Content-Type', 'application/json');
    res.send(JSON.stringify(tweets));
});

var server = app.listen(8000, function () {
    console.log('Started mock server');
});
