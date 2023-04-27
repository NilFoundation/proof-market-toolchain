 #!/bin/bash

PROOF_MARKET_TOOCLHAIN_BRANCH='48-stabil-proof-producer'
PROOF_PRODUCER_USER='skm'
PROOF_PRODUCER_USER_SECRET='skm'

echo $PROOF_PRODUCER_USER > .user
echo $PROOF_PRODUCER_USER_SECRET > .secret

git clone --recurse-submodules https://github.com/NilFoundation/proof-market-toolchain -b $PROOF_MARKET_TOOCLHAIN_BRANCH

docker image  build . -t proof-producer
docker run -it proof-producer-image