import gdb.printing

def call_method(val, method_name):
    addr = str(val.address).split()
    eval_string = "(*(" + str(val.dynamic_type) + "*)(" + addr[0] + "))." + method_name + "()"
    return gdb.parse_and_eval(eval_string)

class AlkValuePrinter(gdb.ValuePrinter):
    "Print an AlkValue"

    def __init__(self, val):
        self.val = val
        d = val['d']['d']
        self.members = [('*d', d.dereference())]

    def to_string(self):
        return call_method(self.val, "toDouble")

    def display_hint(self):
        return 'double'

    def children(self):
        return iter(self.members)

class MPQClassPrinter(gdb.ValuePrinter):
    "Print an mpq_class value"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        num = double(self.val['mp']['_mp_num'])
        denom = double(self.val['mp']['_mp_den'])
        return str(num/denom)

    def display_hint(self):
        return 'string'

try:
    _alkvalue_pretty_printers = gdb.printing.RegexpCollectionPrettyPrinter("Alkimia")
    gdb.printing.register_pretty_printer(None, _alkvalue_pretty_printers)

    _alkvalue_pretty_printers.add_printer('AlkValue', '^AlkValue$', AlkValuePrinter)
    _alkvalue_pretty_printers.add_printer('mpq_class', '^mpq_class', MPQClassPrinter)
    gdb.write("installed pretty printer for Alkimia\n")
except:
    gdb.write("pretty printer for Alkimia already installed\n")

