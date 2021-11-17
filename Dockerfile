FROM tizho/azurertos-micro-ros-agent:node

WORKDIR /workdir
COPY ./service/ /workdir/service/
COPY ./run.sh /workdir/

ENTRYPOINT ["/workdir/run.sh"]
