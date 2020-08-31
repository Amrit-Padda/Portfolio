from blockcypher import subscribe_to_address_webhook, list_webhooks
token = '' #removed for privacy
walletaddress = '' #removed for privacy
ngrok = 'https://0976e752.ngrok.io'
callbackURL = 'https://bitkye.com/callback'

def subscribe():
    subscribe_to_address_webhook(callback_url=' https://77b9a69d.ngrok.io', subscription_address='', event='confirmed-tx', api_key='') #removed for privacy

def webhook_manager():
    webhooks = list_webhooks(token)

    if not webhooks:
        subscribe
    else:
        return
