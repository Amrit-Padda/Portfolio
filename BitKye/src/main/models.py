from src.extensions import db
from datetime import datetime
from werkzeug.security import generate_password_hash, check_password_hash
from flask_login import UserMixin
from hashlib import md5
from time import time


class Pool(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    timestamp = db.Column(db.DateTime, index=True, default=datetime.utcnow)
    tx1 = db.Column(db.JSON)
    tx2 = db.Column(db.JSON)
    tx3 = db.Column(db.JSON)
    tx4 = db.Column(db.JSON)
    tx5 = db.Column(db.JSON)
    tx6 = db.Column(db.JSON)
    tx7 = db.Column(db.JSON)
    tx8 = db.Column(db.JSON)
    tx9 = db.Column(db.JSON)
    tx10 = db.Column(db.JSON)
    winner = db.Column(db.String(50))
    topbid = db.Column(db.VARCHAR(length=36))
    poolval = db.Column(db.VARCHAR(length=60))
    
    def __repr__(self):
        return '<Pool {}>'.format(self.body)

class Queue(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    timestamp = db.Column(db.DateTime, index=True, default=datetime.utcnow)
    addresses = db.Column(db.String(500), unique=True)
    tx = db.Column(db.JSON)
    bid = db.Column(db.VARCHAR(length=36))

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(500), unique=True)




