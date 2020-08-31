from bit import Key, PrivateKey, PrivateKeyTestnet
k = '' #removed for privacy
bitKye_key = Key(k)

def getBalance():
    return bitKye_key.getBalance('cad')

def distribute(transactions):
    bitKye_key.create_transaction(transactions)