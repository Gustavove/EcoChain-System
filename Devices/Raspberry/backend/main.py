import os

from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/init', methods=['GET'])
def init():
    return "Backend information would be send to inicialize the conexión with sensor/node"


