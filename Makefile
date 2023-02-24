## Work path
ifndef WORKSPACE
	export WORKSPACE=./
endif

default: docker-prepare-image toolchain-build

docker-prepare-image:
	docker build -t proof-market-toolchain-docker .  

toolchain-build:
	docker run -it --mount type=bind,src="$(PWD)",target=/proof-market-toolchain proof-market-toolchain-docker  sh ./build.sh

docker-run:
	docker run -it --mount type=bind,src="$(PWD)",target=/proof-market-toolchain proof-market-toolchain-docker  bash