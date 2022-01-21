#!/bin/bash

git add .
git commit -m '.'
git push
ssh -t dhenuh /var/dev/pdfWriter/update.sh