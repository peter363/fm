#!/bin/bash

./keytool-importkeypair -k Sign.jks  -p 111111 -pk8 platform.pk8  -cert platform.x509.pem  -alias SerialDemo
