from src.extensions import socketio, db, limiter
from flask import Flask, render_template, request, Blueprint
from flask_socketio import SocketIO, emit
from src.main.models import Queue, Pool, User
from src.main.dbmanager import wipe, addtransaction, fillPool, recvaddress, POOL_SIZE, buyin, convert, fees, newEmail
from sqlalchemy import func
import json
import datetime as dt
import os
import sys

view_bp = Blueprint('view', __name__)

# Handler for default flask route
@view_bp.route('/')
@limiter.limit("1/second", override_defaults=False)
def index():
    return render_template('index.html')
   
@view_bp.route('/active')
def queues():
    queue = Queue.query.all()
    pool = Pool.query.all()
    return render_template('queue.html', queue = queue, pool = pool, buyin = buyin, buyin2 = (buyin*1000), address = recvaddress[1], fee = fees, curr = 'BTC', curr2 = 'mBTC')

@view_bp.route('/about')
def about():
    queue = Queue.query.all()
    pool = Pool.query.all()
    return render_template('about.html', fee = fees)

@view_bp.route('/callback', methods=['GET', 'POST'])
def callback():
    socketio.emit('new hk', {'data':'new hook'}, broadcast=True)
    if request.method == 'GET':
        return '<h1>Hello</h1>'
    if request.method == 'POST': 
        data = request.get_json(force=True)
        
        if data:                
            addresses, bid, flag = addtransaction(data)
            
            if flag: 
                newBid = {"addresses": addresses, "bid": convert(bid)}
                socketio.emit('new bidder', json.dumps(newBid), broadcast=True)
    
            else:
                return '{"success":"dup"}'
            
            qfill = len(Queue.query.all())

            if qfill == POOL_SIZE:
                fillPool()
                wipe()
                socketio.emit('full pool', {'data':'pool full'}, broadcast=True)
                return '{"success":"newbid"}'
            else:
                return '{"success":"newbid"}'
        else:
            return '{"failed":"no data"}'
                
@view_bp.errorhandler(429)
def page_not_found():
    return render_template('429.html')
# Handler for a message recieved over 'connect' channel
@socketio.on('new email')
def test_connect(email):
    newEmail(email['data'])
    


    
    