#!/bin/bash
dmesg |grep "#### INODE:"|tail -`dmesg |grep "####"|tail -1|cut -d"=" -f2`
