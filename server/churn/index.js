module.exports = function(context, req) {

    var payload1 = {
        "Times":    [ 0,  500,    2500,   3000,   4000,   4500,   6500,   7000 ],
        "Speeds":   [ 0,  100,    100,    0,      0,      -100,   -100,   0    ]
    };
    var payload2 = {
        "Times":    [ 0,  1000,    2000,   3000,   4000 ],
        "Speeds":   [ 0,  100,    -100,    100,      -100]
    };

    var message = serialize(payload);
    context.log('Kefir chef sending recipe %s', message);

    context.res = {
        // status: 200, /* Defaults to 200 */
        body: message
    };
    context.done();
};

function serialize(payload)
{
    var dataSize = payload["Times"].length;
    var dataSize2 = payload2["Times"].length;
    var message = "";
    for (var i = 0; i < dataSize; i++) {
        message += payload["Times"][i];
        message += ":";
        message += payload["Speeds"][i];
        message += ";";
    }
    message += ".";
    for (var i = 0; i < dataSize2; i++) {
        message += payload2["Times"][i];
        message += ":";
        message += payload2["Speeds"][i];
        message += ";";
    }
    message += ".";
    return message;
}