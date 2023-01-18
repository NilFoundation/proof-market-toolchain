---
description: Proof Market front end guide
---

# Front End

{% hint style="info" %}
Please note this product is in beta and expected to change.
{% endhint %}

Proof Market front end can be accessed at : [proof.market](https://proof.market)

## Login&#x20;

<figure><img src="../.gitbook/assets/image (7).png" alt=""><figcaption><p>Login</p></figcaption></figure>

Authenticated users should enter their credentials in the provided fields. &#x20;



{% hint style="info" %}
If you do not have the credentials, users can still view the exchange, but they will be unable to post orders to buy/sell proofs.
{% endhint %}



## Request Access

<figure><img src="../.gitbook/assets/image (3).png" alt=""><figcaption></figcaption></figure>

Click on Request credentials & submit the form with your email address.

<figure><img src="../.gitbook/assets/image.png" alt=""><figcaption></figcaption></figure>

Credentials will be emailed to users.

## Dashboard

<figure><img src="../.gitbook/assets/image (11).png" alt=""><figcaption></figcaption></figure>

The proof market dashboard can be broken down into following components

* [A : Circuit List](front-end.md#circuit-list)
* [B:  Trades](front-end.md#trades)
* [C : Last Proof Producer](front-end.md#last-proof-producer)
* [D : Circuit Details](front-end.md#circuit-details)
* [E : Circuit Price Dashboard](front-end.md#circuit-price-dashboard)
* F: Order Book
* G: Create Orders
* H: Manage Orders

### Circuit List

<figure><img src="../.gitbook/assets/image (9).png" alt=""><figcaption></figcaption></figure>

The circuit list shows the circuits (instruments) which are available on the proof market. Orders can be created to buy or sell proofs for them. A user can search for circuits and the component shows the price change in the last 24 hours .This selection also alters all the other components.



### Trades

<figure><img src="../.gitbook/assets/image (1).png" alt=""><figcaption></figcaption></figure>

The trades component shows all the orders which have been executed on the marketplace for the selected Circuit.

### Last Proof Producer

<figure><img src="../.gitbook/assets/image (10).png" alt=""><figcaption></figcaption></figure>

This section shows the username of the last proof producer on the marketplace.

### Circuit Details

<figure><img src="../.gitbook/assets/image (5).png" alt=""><figcaption></figcaption></figure>

The circuit details component shows more information about the circuit, Aggregated market place stats and some description.

### Circuit Price Dashboard

This comprises of two views

* Proof Cost :  Circuit Price dash board shows a historical view of the price settlement for this circuit.



<figure><img src="../.gitbook/assets/image (4).png" alt=""><figcaption><p>Proof Cost</p></figcaption></figure>

* Proof Generation Time - This shows a historical view of how long did proof generation take from the time the order was matched to the time a proof was submitted to the market

<figure><img src="../.gitbook/assets/image (6).png" alt=""><figcaption><p>Proof Generation time</p></figcaption></figure>



### Create Orders

Create Orders  section can be used to place the following orders

1. **Buy Order** : A buy order is posted by a proof requester. Each buy order comprises of the following parameters:
   1. **Cost** : Amount the user is willing to pay for the proof. This is a number represented in USD.
   2. **Order Time Out** : Once the order enters the market place, how long should it stay in the order book. If it times out before being matched , it is purged. This is a number representing   Minutes.
   3. **Generation Time**:  Once the order is matched, this specifies the maximum time the proof generator has to publish the proof. This is an optional parameter. This is a number expressed in minutes.&#x20;
   4. **Public Inputs :** This is an optional parameter to add any public inputs if the circuit requires it.

<figure><img src="../.gitbook/assets/image (3) (1).png" alt=""><figcaption><p>Buy Order</p></figcaption></figure>

****

2\. **Sell Order**: A sell order is posted by a proof Generator. Each sell order comprises of the following parameters

* **Cost:** Amount the proof generator is willing to accept to create a proof.
* **Order Time Out** : Once the order enters the market place, how long should it stay in the order book. If it times out before being matched , it is purged. This is a number representing   Minutes.
* **Generation Time:** The upper bound by which the proof will be generated once matched.

<figure><img src="../.gitbook/assets/image (5) (1).png" alt=""><figcaption><p>Sell Order</p></figcaption></figure>

### Manage Orders

Under manage orders a user can do the following

* **Active Orders :** This view shows all the orders of the users which are currently active and have not been matched yet. A user can cancel an active order from this screen.

<figure><img src="../.gitbook/assets/image (6) (1).png" alt=""><figcaption></figcaption></figure>

* **History :** This view shows all the historical orders a user has placed.

### Order Book

The order book shows the bid and asks on the market place for orders which have not yet matched.

<figure><img src="../.gitbook/assets/image (8).png" alt=""><figcaption></figcaption></figure>



