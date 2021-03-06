#!/usr/bin/env python3
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

"""
Demonstrates the client side of the synchronous request-response pattern
(also known as RPC or Remote Procedure Call) using proton.

"""
import optparse
from proton import Message, Url
from proton.utils import SyncRequestResponse, BlockingConnection

parser = optparse.OptionParser(usage="usage: %prog [options]",
                               description="Send requests to the supplied address and print responses.")
parser.add_option("-a", "--address", default="localhost:5672/examples",
                  help="address to which messages are sent (default %default)")
parser.add_option("-t", "--timeout", type="float", default=5,
                  help="Give up after this time out (default %default)")
opts, args = parser.parse_args()

url = Url(opts.address)
client = SyncRequestResponse(BlockingConnection(url, timeout=opts.timeout), url.path)

try:
    REQUESTS = ["Twas brillig, and the slithy toves",
                "Did gire and gymble in the wabe.",
                "All mimsy were the borogroves,",
                "And the mome raths outgrabe."]
    for request in REQUESTS:
        response = client.call(Message(body=request))
        print("%s => %s" % (request, response.body))
finally:
    client.connection.close()
