# Economics

{% hint style="danger" %}
### Disclaimer

This is the first version of =nil; Proof Market economics description. The constant values as well as fee, penalties and rating politics may be changed in the future according to real-life data and community requests.
{% endhint %}

Proof Market constants:

| Name       | Value | Meaning                                |
| ---------- | ----- | -------------------------------------- |
| _pf_       | 10    | penalty factor                         |
| _c_        | 0.5   | commission rate                        |
| _mc_       | 1     | minimal commission value               |
| _app\_dev_ | 0.7   | application developers part of the fee |
| _cir\_dev_ | 0.3   | circuit developers part of the fee     |

### Currency

For now, main and the only one accepted currency at =nil; Proof Market is the native token of =nil; Cluster.

### Basic Workflow

1. A proof buyer sends a bid order to the market with a cost _c\_b_;
2. Proof Market locks _c\_b_ tokens of the buyer;
3. A proof producer sends an ask order to the market with a cost _c\_a <= c\_b_;
4. Proof Market matches bid and ask order;
5. The proof producers generates a proof and sends it to the market;
6. Proof Market verifies proof and pays _c\_b - commission_ tokens to the producer;
7. The proof buyer uses takes their proof and uses it in some way.

### Orders status

Bids (order for buying proofs) and asks (orders for selling proofs) have the following statuses:

* _Created_: order was sent to the marketplace and was not matched (i.e. there is no buyer for an ask or proof producer for a bid);
* _Processing_: order was matched and Proof Market is waiting while proof will be generated;
* _Completed_: proof was generated and verified by Proof Market;
* _Withdrawn_: order was cancelled before matching or after a) wrong proof was submitted or b) prof was not submitted in the requested time interval.

### Bid/Ask Matching process

Proof Market provides more complex behaviour than one-hop trading. Each deal includes a proof generation stage. This stage may result in the completed deal (proof was provided and verified by Proof Market) or a restart of the matching (if the proof has not been provided or verification has failed). Moreover, some applications require generating proofs in some time interval. Thus, in the future additional parameters will influence the matching algorithm:

* Proof generation time. Proof producers provide time bounds _t\_a_ in which they are able to generate proof for the statement. Users can set the desired proof generation time _t\_b_. Only asks with the _t\_a_ ≤ _t\_b_ can be matched with the user’s bid.
* Proof producers rating. The rating depends on the rate of the completed orders by the proof producers (i.e. the proof producer has provided correct proof in the requested time interval).

Here are the basic rules of order matching for bid with desired time _t\_b_ and price _p\_b_:

1. Choose asks with _t\_a_ ≤ _t\_b_ and _p\_a_ ≤ _p\_b_.
2. Sort them by price and proof generation time (priority between these parameters is defined by the user’s bid).
3. Apply proof producers rating to the sorted list.
4. Choose the top ask from the list.

### Bids & Asks resale

When creating a new bid, there may be situations when there is already a equivalent bid in the database. Two bids are considered equivalent if they match in **statement** and **public\_input** fields. Therefore, when adding a new bid, the application first tries to find an equivalent bid in the system. If such a bid is found and there is already a proof for it the following happens:

* the new bid immediately goes into the **completed** state;
* the proof-requester pay commission to the proof-market;
* the proof-requester pay commission to the proof-producer who generated the proof for existing bid;
* the proof-requester pay commission to the owner of the existing bid;
* unspent tokens are returned to the proof requester.

It is assumed that the total amount of all commissions will be significantly lower than the market price of the bid. This approach allows a proof-requester to recoup its bid costs in the future (if he is the owner of the bid whose result will be reused) and even earn. Similarly for a proof-producer, his proof can be sold more than once and the proof-producer will receive his commission from each such additional sale.

### Registration

#### Proof Requester

There is no process of registration for proof buyers. Any =nil; Cluster account is able to send a bid to the marketplace. The only requirement is that user's account must have enough funds to pay for the order.

#### Proof Producer

Account has to register as a Proof Producer to be able to set asks to the marketplace. The process includes locking funds (_deposit_). The number of locked funds reflects the number of orders that can be processed by the proof producer at the moment.

Let _X_ is the number of locked funds by the proof producer. Then they can process bids on the sum of _X/pf_ at once. This restriction is related to the penalties policy described in the next sections.

### Funds transferring & commissions

When placing a bid, the customer sets the cost that he is willing to pay for the successfully submitted proof for the bid. If the proof for the bid is provided on time, the amount indicated in the ask cost is deducted from the customer's account and credited to the account of the proof producer.

_c_% of the bid cost goes as the commission but not less than _mc_ tokens. It is split between two parties

* _app\_dev_ part goes to the Proof Market development team;
* _cir\_dev_ part goes to Circuit Developer of the ordered statement.

#### Funds withdraw

**From proof requester perspective:**

Each time a new bid is added, an amount equal to the bid cost is locked on the account. If the bid is successfully closed, the locked amount is debited from the account. If the bid is not closed within the available time, the customer has the opportunity to withdraw the bid. If the bid is successfully withdrawn, the locked amount will be unlocked.

**From proof producer perspective:**

At any moment a proof producer is able to withdraw funds that are not part of the deposit for their active asks. This means that at each moment the proof producer has pf∗S locked funds where S is the sum of all their asks that are not completed or withdrawn.

### Ratings

In the system, each proof producer has a rating. Rating is taken into account in many different processes, for example in matching, ask placement, etc. For simplicity, the conscientious fulfilment of obligations increases the rating and thereby increases the likelihood of a bid matching, while the producer’s dishonest work can lead to a decrease in ratings and various restrictions.

**Rating System is in active design/development stage, more details on it will be provided soon**

### What happens if the proof is not submitted on time

The situation when the proof is not provided on time or is not provided at all is considered undesirable. This situation may have a negative impact on the producer's rating in the first place. Regular non-fulfilment of undertaken obligations, in addition to downgrading the rating, may have the following consequences:

* Account blocking
* Limits on the allowed number of assigned bids
* Setting a limit on the minimum generation time for the producer

Ιf the proof was not provided on time, the bid can be unmatched and reassigned to another proof producer.

Proof producers do not get the payment for the proofs that was not provided in the requested time interval. However, there is a difference in rating change for the cases when proof was not provided on time and proof was not provided at all. Remind that proof producers define their minimum proof generation time and the matching algorithm never matches asks to the bids with tb\<ta.

### The delay between the bid/ask submitting and the matching

There is no delay between bid/ask submitting and running the matching algorithm. It means that as soon as bid/ask was submitted, Proof Market tries to match it with other existing orders.

### Penalties

There are two types of penalties to proof producers:

* Rating decreasing. It happens when proof was not provided on time or was not provided at all.
* Fine on the locked funds. It happens when verification fails for the provided proof, then the proof producer pays _pf \* c_ tokens as a fine where _c_ is the order's cost. It is considered malicious behaviour from the proof producer side.

There are no penalties for proof buyers because they use a one-hop pipeline that excludes malicious behaviour from their side.
