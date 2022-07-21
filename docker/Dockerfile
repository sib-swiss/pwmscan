FROM ubuntu:20.04 AS builder

ARG PWMSCAN_VERSION
ARG BUILD_DATE

# METADATA
# Use Open Containers Initiative (OCI)
# See https://rehansaeed.com/docker-labels-depth/
# See https://github.com/opencontainers/image-spec/blob/master/annotations.md
# Exist also another structured label initiative: http://label-schema.org/ superceded by OCI now
LABEL org.opencontainers.image.title="A genome-wide Position Weight Matrix (PWM) Scanner"
LABEL org.opencontainers.image.version=$PWMSCAN_VERSION
LABEL org.opencontainers.image.vendor="SIB Swiss Institute of Bioinformatics"
LABEL org.opencontainers.image.authors="sebastien.moretti@sib.swiss"
LABEL org.opencontainers.image.url="https://ccg.epfl.ch/pwmtools/"
LABEL org.opencontainers.image.source="https://sourceforge.net/projects/pwmscan/"
LABEL org.opencontainers.image.documentation="https://sourceforge.net/projects/pwmscan/"
LABEL org.opencontainers.image.licenses="GPLv3"
LABEL org.opencontainers.image.description="PWMScan is used to scan a position weight matrix (PWM) against a genome or, in general,\
a large set of DNA sequences. The PWM is the most commonly used mathematical \
model to describe the DNA binding specificity of a transcription factor (TF). \
A PWM contains scores for each base at each position of the binding site. The \
TF binding score for a given k-mer sequence is then obtained by simply adding \
up the base-specific scores at respective positions of the binding site. \
PWMScan takes as input a PWM, the background probabilities for the letters of \
the DNA alphabet, and a threshold score or a p-value. The search is carried out \
across the entire genome sequence. It can accept PWMs, such as those available \
in the Transfac or Jaspar databases as well as plain-text PWMs. It computes all \
occurrences of the PWM in the genome sequence for a given p-value threshold or \
cut-off. The match list is provided in various genome annotation formats, \
including BEDdetail, FPS, and SGA."
LABEL org.opencontainers.image.created=$BUILD_DATE

#FIXME Does it need bowtie indexes in the container?
#FIXME Should we define bowtieDir and workDir variables here?

# INSTALLATION
ENV USER=pwmscan
# In a single layer to save time AND space
RUN echo '# OS update' \
	&& export DEBIAN_FRONTEND=noninteractive \
	&& apt-get update  -y \
	&&        echo '# Install OS requirements' \
	&& apt-get install -y --no-install-recommends wget unzip ca-certificates make gcc libc6-dev man-db perl libmath-round-perl libscalar-util-numeric-perl libscalar-list-utils-perl python2 \
	&&        echo '# Install PWMScan' \
	&& cd /usr/local/ \
	&& wget 'https://gitlab.sib.swiss/EPD/pwmscan/-/archive/master/pwmscan-master.tar.gz' \
	&& tar xvfz pwmscan-master.tar.gz \
	&& rm -f pwmscan-master.tar.gz \
	&& cd pwmscan-master/ \
	&& make clean \
	&& make \
	&& sed -i 's@^binDir = .*@binDir = /usr/local/bin@' Makefile \
	&& sed -i 's@#!.*perl@#!/usr/bin/perl@' perl_tools/*.pl \
	&& make install \
	&& cd /usr/local/ \
	&& rm -Rf /usr/local/pwmscan-master/ \
	&&        echo '# Install PWMScan requirements: ChIP-Seq' \
	&& cd /usr/local/ \
	&& wget 'https://gitlab.sib.swiss/EPD/chipseq/-/archive/master/chipseq-master.tar.gz' \
	&& tar xvfz chipseq-master.tar.gz \
	&& rm -f chipseq-master.tar.gz \
	&& cd chipseq-master/ \
	&& echo '#Fix math round compilation issue' \
	&& sed -i 's@\((LDFLAGS).*\)@\1 -lm@'                 tools/Makefile \
	&& echo '#Fix manpath' \
	&& sed -i 's@share/man/chip-seq/man1@share/man/man1@' Makefile \
	&& export DESTDIR=/usr/ \
	&& make clean \
	&& make progs \
	&& cd tools/ \
	&& make \
	&& cd .. \
	&& make install \
	&& echo '#Fix Perl bang path' \
	&& sed -i 's@#!.*perl@#!/usr/bin/perl@' tools/*.pl \
	&& install tools/*.pl $DESTDIR/bin/ \
	&& make install-man \
	&& mandb -c \
	&& mv -f /usr/bin/man.REAL /usr/bin/man \
	&& make install-dat \
	&& make install-doc \
	&& cd .. \
	&& rm -Rf chipseq-master/ \
	&&        echo '# Install PWMScan requirements: bowtie' \
	&& cd /usr/local/ \
	&& echo '#This particular version is used on the https://ccg.epfl.ch/pwmtools/ server. Do not know if more recent versions work' \
	&& wget 'https://github.com/BenLangmead/bowtie/releases/download/v1.2.2_p1/bowtie-1.2.2-linux-x86_64.zip' \
	&& unzip bowtie-*-linux-x86_64.zip \
	&& rm -f bowtie-*-linux-x86_64.zip \
	&& cd bowtie-*-linux-x86_64/ \
	&& sed -i 's@#!.*python@#!/usr/bin/python2@' bowtie bowtie-build bowtie-inspect \
	&& install bowtie* scripts/* /usr/local/bin/ \
	&& cd /usr/local/ \
	&& rm -Rf bowtie-*-linux-x86_64/ \
	&&        echo '# TEST' \
	&& matrix_prob --help || true \
	&& echo \
	&& bowtie --version \
	&& echo \
	&& chipcenter -h || true \
	&&        echo '# CLEANING' \
	&& echo '## So try to remove manually useless stuff' \
	&& apt remove -y *-dev git wget cmake make swig autoconf check gcc g++ gfortran python3* patchelf unzip \
	&& apt autoremove -y \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/* \
	&& groupadd -g 1001 ${USER} && useradd -r -u 1001 -g ${USER} ${USER}


# SECURITY
## Control root access
USER ${USER}
ENV HOME=/home/${USER}
WORKDIR ${HOME}
##Rest of Dockerfile with this user
ENV LC_ALL="C"


CMD ["/usr/local/bin/matrix_prob", "--help"]

