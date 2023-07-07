# Economics

This is the first version of `=nil;` Proof Market economics description.
The constant values, fees, penalties, and rating politics are subject to changes
in the future according to real-life data and community requests.

Proof Market constants:

| Name       | Value | Meaning                                 |
| ---------- | ----- | --------------------------------------  |
| _pf_       | 10    | penalty factor                          |
| _c_        | 0.5   | commission rate                         |
| _mc_       | 1     | minimal commission value                |
| _app\_dev_ | 0.7   | application developer's part of the fee |
| _cir\_dev_ | 0.3   | circuit developer's part of the fee     |

## Basic workflow

1. A proof requester sends a request with a desired price _c\_r_ to the market.
2. The Proof Market locks _c\_r_ tokens from the buyer's account.
3. Proof producers send proposals to the market with a price _c\_p <= c\_r_.
4. The Proof Market matches the request with the proposal of the proof producer.
5. The proof producer generates a proof and sends it to the market.
6. The Proof Market verifies proof and pays _c\_r - commission_ tokens to the producer.
7. The proof requester takes their proof and uses it.

## Orders status

Requests (orders for buying proofs) and proposals (orders for selling proofs) have the following statuses:

* _Created_: order was sent to the marketplace and was not yet matched,
  meaning there is no buyer for a proposal or no proof producer for a request.
* _Processing_: order was matched, and the Proof Market is waiting for the proof generator
  to provide the proof.
* _Completed_: the proof was generated and verified by Proof Market.
* _Withdrawn_: the order was canceled before matching, a wrong proof was submitted, or
  no proofs were submitted in the requested time interval.

## Request/proposal matching process

The Proof Market provides a more complex behavior than one-hop trading.
Each deal includes the proof generation stage.
This stage may result in a completed deal if proof was provided and verified by the Proof Market
or a restart of the matching if no proof has been provided or proof's verification has failed.
Moreover, some applications require generating proofs in the specified time interval.
In the future, the following parameters will influence the matching algorithm:

* Proof generation time.
  Proof producers provide time bounds _t\_p_ in which they can generate proof for the statement.
  Users can set the desired proof generation time _t\_r_.
  Only proposals with _t\_p_ ≤ _t\_r_ can be matched with the user's request.
* Proof producer's rating.
  Proof producer's rating depends on the rate of completed orders — orders where the proof producer
  has provided correct proof in the requested time interval.

Here are the basic rules of order matching for a request with specified time _t\_r_ and price _c\_r_:

1. Choose proposals with _t\_p_ ≤ _t\_r_ and _c\_p_ ≤ _c\_r_.
2. Sort them by price and proof generation time.
   The user's request defines the priority of these parameters.
3. Apply proof producer's rating to the sorted list.
4. Choose the top proposal from the list.

## Proof resale

There may be situations when an equivalent request is already in the database.
Two requests are considered equivalent if they match in the **statement** and **public\_input** fields.
Upon receiving a new request, the application tries to find an equivalent request in the system.
This is what happens if such a request is found and there's already proof for it:

* The new request immediately goes into the **completed** state.
* The proof requester pays commissions to the Proof Market, the circuit developer,
  the proof producer who generated the proof for the existing request, and the proof requester
  of the original request.
* Unspent tokens are returned to the proof requester.

The total amount of all commissions should be significantly lower than the market price of the request.
This approach allows the proof requester, whose proof will be reused, to recoup proof costs
in the future and maybe even earn something on it.
Similarly, a proof producer can sell their proof more than once and receive a commission
from each such additional sale.

## Registration
### Proof requester

There is no process for proof requesters registration.
Any `=nil;` cluster account can send a request to the marketplace.
The only requirement is that the user's account must have enough funds to pay for the order.

### Proof producer

Account has to register as a Proof Producer to put proposals on the marketplace.
The process includes locking funds (_deposit_).
The number of locked funds reflects the number of orders that the proof producer can process at the moment.

Let _X_ be the number of locked funds by the proof producer.
Then they can process requests on the sum of _X/pf_ at once.
This restriction is related to the penalties policy described in the following sections.

## Funds transferring and commissions

When placing a request, the proof requester sets the cost they are willing to pay for
the successfully submitted proof.
If the proof for the request is provided on time, the price indicated in the proposal cost
is deducted from the proof requester's account and credited to the account of the proof producer.

_c_% of the request cost goes as the commission, but not less than _mc_ tokens.
It's split between two parties:

* _app\_dev_ part goes to the Proof Market development team;
* _cir\_dev_ part goes to the circuit developer of the ordered statement.

### Funds withdrawal

#### From the proof requester's perspective

Each time a new request is added, an amount equal to the request cost is locked on the account.
The locked funds are debited from the account once the request is successfully closed.
If the request is not completed within the specified time interval, the proof requester
can withdraw the request.
If the request is successfully withdrawn, the locked funds will be unlocked.

#### From the proof producer's perspective

At any moment, a proof producer can withdraw funds that are not part of the deposit
for their active proposals.
This means that the proof producer has _pf∗S_ locked funds where _S_ is the sum of all
their proposal orders that are not completed or withdrawn.

## Ratings

Each proof producer has a rating in the system.
Rating impacts many processes, such as matching, proposal placement, and others.
For simplicity, the conscientious fulfillment of obligations increases the rating.
It increases the likelihood of a request matching, while the producer's dishonest work
can lead to decreased ratings and various restrictions.

{% hint style="info" %}
The rating system is in the design/development stage; more details on it will be provided soon**
{% endhint %}

## What happens if the proof is not submitted on time

The situation when the proof is not provided on time or is not provided at all
is considered undesirable.
Requests may be unmatched and reassigned to another proof producer in such cases.
Proof producers don't receive payments for proofs not provided in the requested time interval
and are subject to penalties and restrictions.
Remember that proof producers define their minimum proof generation time,
and the matching algorithm never matches proposals to the requests with _t\_r_ < _t\_p_.

### Penalties

There are two types of penalties for proof producers:

* If a proof producer provides proof out of time or doesn't provide it at all,
  their rating will decrease.
  However, there's a difference in rating change for when proof was not provided on time
  and when it was not provided at all.
* If a proof producer provides proof that doesn't verify, a fine of _pf \* c_ tokens
  will be subtracted from their deposit, where _c_ is the order's cost.

In addition to that, regular non-fulfillment of undertaken obligations
may have the following consequences:

* account blocking;
* limits on the allowed number of assigned requests;
* a limit on the minimum generation time for the producer.

There are no penalties for proof requesters because they use a one-hop pipeline that excludes
malicious behavior from their side.

## The delay between the request/proposal submission and the matching

There is no delay between the request/proposal submitting and running the matching algorithm.
As soon as the request/proposal is submitted, the Proof Market tries to match it with
other existing orders.
