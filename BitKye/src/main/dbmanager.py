import json
from sqlalchemy import func, exc
from src.extensions import db
from src.main.models import Queue, Pool, User
import sys
from bit import wif_to_key
from blockcypher import simple_spend

priv = '---' #removed for privacy
wallet = wif_to_key(priv)
recvaddress = ['', ''] #removed for privacy
key = '' #removed for privacy
POOL_SIZE = 10
buyin =  0.0005
fees = 1

def get_count(q):
    count_q = q.statement.with_only_columns([func.count()]).order_by(None)
    count = q.session.execute(count_q).scalar()
    return count

def addtransaction(data):
    txaddresses = []
    bidValue = 0
    
    for txInput in data["inputs"]:
        txaddresses.append(txInput["addresses"])


    for output in data["outputs"]:
        for addressin in output["addresses"]:
            if addressin in recvaddress:
                bidValue = output["value"]
            #bidValue = output["value"]

    newEntry = Queue(tx=data, bid=bidValue, addresses = txaddresses[0])
    try:
        db.session.add(newEntry)
        db.session.commit()
        return txaddresses[0], bidValue, True
    except exc.IntegrityError:
        db.session.rollback()
        return 0,0,False


def convert(btc):
    bit = float(btc) / 100000000.0
    return bit

def revert(sat):
    btc = int(sat * 100000000)
    return btc

def fillPool():
    currentPool = Queue.query.order_by(Queue.bid.desc()).all()
    poolValue = 0.0

    for entry in currentPool:
        poolValue = poolValue + int(entry.bid)

    interestval = revert((int(currentPool[0].bid) - buyin))
    interest = (interestval / (POOL_SIZE - 1))
    winnerval = revert(poolValue - interest)
    empty = json.dumps({"addresses": "none"})

    newPool = Pool(tx1=currentPool[0].tx, 
                    tx2=currentPool[1].tx,
                    tx3=currentPool[2].tx, 
                    tx4=currentPool[3].txty,
                    tx5=currentPool[4].tx, 
                    tx6=currentPool[5].tx,
                    tx7=currentPool[6].tx, 
                    tx8=currentPool[7].tx,
                    tx9=currentPool[8].tx, 
                    tx10=currentPool[9].tx,
                    winner = currentPool[0].addresses,
                    topbid = currentPool[0].tx["outputs"][0]["value"],
                    poolval = poolValue)

    db.session.add(newPool)
    db.session.commit()

def wipe():
    wipe = Queue.query.delete()
    db.session.commit()


def newEmail(emailIn):
    try:
        em = User(email=emailIn)
        db.session.add(em)
        db.session.commit()
    except exc.IntegrityError:
        db.session.rollback()
  


"""
    wallet.create_transaction([(currentPool[0].addresses, winnerval, 'btc'),
                                (currentPool[1].addresses, interest, 'btc'),
                                (currentPool[2].addresses, interest, 'btc'),
                                (currentPool[3].addresses, interest, 'btc'),
                                (currentPool[4].addresses, interest, 'btc'),
                                (currentPool[5].addresses, interest, 'btc'),
                                (currentPool[6].addresses, interest, 'btc'),
                                (currentPool[7].addresses, interest, 'btc'),
                                (currentPool[8].addresses, interest, 'btc'),
                                (currentPool[9].addresses, interest, 'btc')])

    newPool = Pool(tx1=currentPool[0].tx, 
                    tx2=currentPool[1].tx,
                    tx3=currentPool[2].tx, 
                    tx4=currentPool[3].tx,
                    tx5=currentPool[4].tx, 
                    tx6=currentPool[5].tx,
                    tx7=currentPool[6].tx, 
                    tx8=currentPool[7].tx,
                    tx9=currentPool[8].tx, 
                    tx10=currentPool[9].tx,
                    winner = currentPool[0].addresses,
                    topbid = currentPool[0].tx["outputs"][0]["value"],
                    poolval = poolValue)


    simple_spend(from_privkey=priv, to_address=currentPool[0].addresses[0], to_satoshis=winnerval, coin_symbol='btc-testnet', api_key=key)
    for x in range(1,len(currentPool)):
        simple_spend(from_privkey=priv, to_address=currentPool[x].addresses[0], to_satoshis=interest, coin_symbol='btc-testnet', api_key=key)

    wallet.create_transaction([(currentPool[0].addresses, winnerval, 'btc'),
                                (currentPool[1].addresses, interest, 'btc')])
"""

