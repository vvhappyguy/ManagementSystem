import redis
from flask import Flask

app = Flask(__name__)

r = redis.Redis(host='localhost', port=49153, db=0, password='redispw', username='default')

@app.route('/')
def hello():
    output = 'Temperature = ' + str(r.get('temp')) + '<br>Last updated at ' + str(r.get('temp_ts')) + '<br><br>' + "Humidity = " + str(r.get('hum') + '<br>Last updated at ' + str(r.get('hum_ts')))
    print(output)
    return output

if __name__ == '__main__':
    app.run(host="192.168.1.68", port=8080, debug=True)
