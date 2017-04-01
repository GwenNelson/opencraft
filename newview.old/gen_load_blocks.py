import json
import pprint

fd        = open('1.11.json')
json_data = json.load(fd)
fd.close()

block_data = json_data[0]['blocks']['block']

for k,v in block_data.items():
    print "add_blockload(\"blocks/%s.png\", %s);" % (v['text_id'], v['numeric_id'])
