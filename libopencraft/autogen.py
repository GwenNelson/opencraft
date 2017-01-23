import json
from quarry.data import packets as q_packets

fd = open('burger_data/1.11.json','rb')
json_data = json.loads(fd.read().strip('\n'))
fd.close()

packets_names  = {} # map packet class names to shorter names
packets_fields = {} # map packet class names to lists of field data types
packets_idents = {} # map packet class names to packet ID integers

for k,v in json_data[0]['packets']['packet'].items():
    protover  = 315
    protomode = v['state'].lower()
    if v['direction']=='CLIENTBOUND':
       packdir='downstream'
    else:
       packdir='upstream'
    pack_id = v['id']
    key = (protover,protomode,packdir,pack_id)
    packet_name = 'unknown_%s_%s_%s' % (packdir,protomode,pack_id)
    if q_packets.packet_names.has_key(key):
       packet_name = q_packets.packet_names[key]
    packfields = []
    for field in v['instructions']:
        if field.has_key('type'):
           packfields.append(field['type'])
        else:
           packfields.append('bytes')
    k = '%s_%s_%s' % (packet_name,protomode,packdir)
    packets_names[k]  = packet_name
    packets_fields[k] = packfields
    packets_idents[k] = pack_id

alphabet ='abcdefghijklmnopqrstuvwxyz'

header_fd = open('include/packets.autogen.h','w')
cpp_fd    = open('src/packets.autogen.cpp','w')

header_fd.write("// AUTOGENERATED - See autogen.py, do not modify directly\n")
header_fd.write("#pragma once\n")
header_fd.write("#include <string>\n")
header_fd.write("#include <tuple>\n")
header_fd.write("#include <base_packet.h>\n")
header_fd.write("namespace opencraft {\n namespace packets {\n")

cpp_fd.write("// AUTOGENERATED - See autogen.py, do not modify directly\n")
cpp_fd.write("#include <base_packet.h>\n")
cpp_fd.write("#include <packets.autogen.h>\n")
cpp_fd.write("namespace opencraft {\n namespace pacekts {\n")

# we need this cos burger isn't perfect
# basically we override what burger said with manually defined types
overrides = {'login_success_login_downstream':('login_success',0x02,['string8','string8'])}

for k,v in overrides.items():
    packets_names[k]  = v[0]
    packets_idents[k] = v[1]
    packets_fields[k] = v[2]

mctypes = {'boolean' :'bool',
           'byte'    :'char',
           'byte[]'  :'char*',
           'char'    :'char',
           'string16':'std::string',
           'double'  :'double',
           'float'   :'float',
           'int'     :'int32_t',
           'long'    :'int64_t',
           'short'   :'int16_t',
           'string8' :'std::string',
           'varint'  :'int32_t',
           'position':'std::tuple<float,float,float>',
           'enum'    :'int32_t',
           'bytes'   :'char*'}


for k,v in packets_fields.items():
    skip = False
    params = []
    for f in xrange(len(v)):
        if not mctypes.has_key(v[f]):
           print 'WARNING: Can not autogenerate %s due to missing type %s ' % (k,v[f])
           print "         %s(%s)" % (k,','.join(v))
           skip = True
        else:
           params.append('%s %s' % (mctypes[v[f]],alphabet[f]))
    if not skip:
       # write out the header first
       header_fd.write("class %s : public opencraft_packet {\n" % k)
       header_fd.write("  public:\n")
       header_fd.write("    %s(%s);\n" % (k,','.join(params)))
       header_fd.write("    std::string name() { return \"%s\"; }\n" % packets_names[k])
       for p in params:
           header_fd.write("    %s;\n" % p)
       header_fd.write("  private:\n");
       header_fd.write("};\n")

       # then write out the class implementation


cpp_fd.write("}\n}\n");       
header_fd.write("}\n}\n");
header_fd.close()



