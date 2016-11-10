using System.IO;
using System.Net;
using ProtoBuf;

public static async Task<HttpResponseMessage> Run(HttpRequestMessage req, TraceWriter log)
{
    log.Info($"iot-churner function activated by {req.RequestUri}");

    var payload = new Payload
    {
        Times = new int[] {     0,  500,    2500,   3000,   4000,   4500,   6500,   7000    },
        Speeds = new int[] {    0,  100,    100,    0,      0,      -100,   -100,   0       },
    };
    using (var data = new MemoryStream())
    {
        Serializer.Serialize(data, payload);
        var sb = new StringBuilder();
        foreach (var b in data.ToArray())
        {
            sb.Append((char)b);
        }
        var text= sb.ToString();
        log.Info($"Returning {text}");
        return req.CreateResponse(HttpStatusCode.OK, text);
    }   
}

[ProtoContract]
class Payload
{
    [ProtoMember(1)]
    public int[] Times { get; set; } = new int[10];

    [ProtoMember(2)]
    public int[] Speeds { get; set; } = new int[10];
}