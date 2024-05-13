import random
import time
from flask import Flask, request, jsonify

ADDRESS = "127.0.0.1"
PORT = 5000


app = Flask(__name__)

@app.post("/seed")
def setSeed():
    if request.is_json:
        seed = request.get_json()
        if(seed == "none"):
            random.seed(time.time())
        else:
            random.seed(seed)
        return jsonify("success")
    return {"error" : "Invalid seed"}, 400

@app.post("/generateOne")
def generateNum():
    data = request.get_json()
    print(data)
    min = 0
    max = 0
    if "max" in data.keys():
        max = data["max"]
    else:
        return {"error" : "No maximum value given"}, 400
    if "min" in data.keys():
        min = data["min"]

    try:
        return jsonify(random.randrange(min, max + 1))
    except:
        return {"error" : "Invalid request"}, 400

@app.post("/generateList")
def generateList():
    data = request.get_json()
    print(data)
    amount = 0
    min = 0
    max = 0

    if "amount" in data.keys():
        amount = data["amount"]
    else:
        return {"error" : "No amount of numbers given"}, 400
    if "max" in data.keys():
        max = data["max"]
    else:
        return {"error" : "No maximum value given"}, 400
    if "min" in data.keys():
        min = data["min"]

    result = []
    for i in range(amount):
        result.append(random.randrange(min, max + 1))

    return jsonify(result), 200

app.run(host=ADDRESS, port=PORT)