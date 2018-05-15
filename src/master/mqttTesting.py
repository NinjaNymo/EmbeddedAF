#   _____ _____ _____
#  |   | |__   |   | | EmbeddedAF/src/master/mqttTesting.py
#  | | | |   __| | | | Nikolai Nymo
#  |_|___|_____|_|___| 19-04-18
import paho.mqtt.client as mqtt

#   Call back function when connected to broker:
def connected_cb(client, userdata, flags, rc):
    print('Connected to broker with result code = ' + str(rc))
    client.subscribe('test')

#   Call back function when received PUBLISH message:
def message_cb(client, userdata, msg):
    print(str(msg.payload) + ' publsished in topic ' + msg.topic)

#   Instantiate client:
client = mqtt.Client()
client.on_connect = connected_cb # Sets connected call back function
client.on_message = message_cb   # Sets publish message call back function

#   Connect client to broker:
client.connect('192.168.0.104', 1883, 60)
print('client connected')
client.loop_forever()

