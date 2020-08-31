curl -H "Content-Type: Application/json" --data-binary  @C:\Users\Amrit\Documents\GitHub\BitKye\test\webhook.json https://api.blockcypher.com/v1/btc/test3/hooks?token=b09ab54de88b480e9ee023d935527ba7 -v
curl https://api.blockcypher.com/v1/btc/main/hooks?token=ca06d068171c4cae960a52f739ef22f8
curl -X DELETE -Is https://api.blockcypher.com/v1/btc/test3/hooks/343e1aed-226f-4543-abb4-5ae46c3f81a2?token=ca06d068171c4cae960a52f739ef22f8
