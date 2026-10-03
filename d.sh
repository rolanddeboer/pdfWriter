#!/bin/bash

git add .
git commit -m '.'
git push
ssh -t ibu /var/dev/pdfWriter/update.sh