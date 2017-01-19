module.exports = function(context, req) {

    var payload1 = {
        "TotalTime": 30000,
        "Times":    [ 0,  500,    2500,   3000,   4000,   4500,   6500,   7000 ],
        "Speeds":   [ 0,  100,    100,    0,      0,      -100,   -100,   0    ]
    };
    var payload2 = {
        "TotalTime": 10000,
        "Times":    [ 0,  1000,    2000,   3000,   4000 ],
        "Speeds":   [ 0,  100,    -100,    100,      -100]
    };

    var message = serialize(payload1, payload2);
    context.log('Kefir chef sending recipe %s', message);

    context.res = {
        // status: 200, /* Defaults to 200 */
        body: message
    };
    context.done();
};

function serialize(payload1, payload2)
{
    var dataSize = payload1["Times"].length;
    var dataSize2 = payload2["Times"].length;
    var message = ";;;";
    message += payload1["TotalTime"];
    message += ";";
    for (var i = 0; i < dataSize; i++) {
        message += payload1["Times"][i];
        message += ":";
        message += payload1["Speeds"][i];
        message += ";";
    }
    message += ".";
    message += payload2["TotalTime"];
    message += ";";
    for (var i = 0; i < dataSize2; i++) {
        message += payload2["Times"][i];
        message += ":";
        message += payload2["Speeds"][i];
        message += ";";
    }
    message += ".";
    return message;
}