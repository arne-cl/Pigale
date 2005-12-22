BEGIN {
}

/#define PROP_[^<]*<[^\\]*\\c/ {
if ($6!="" && $6!="anything")
  { type=$6; if (($7~/.*>/) || ($7~/\*.*/)) type=type " " $7;
  print "#define TYPE_" $2 "    " type
  }
}
END {
  print "#define GPROP(_gc,_name,_var) Prop1<TYPE_PROP_##_name > _var(_gc.Set(),PROP_##_name)"
  print "#define VPROP(_gc,_name,_var) Prop<TYPE_PROP_##_name > _var(_gc.Set(tvertex()),PROP_##_name)"
  print "#define EPROP(_gc,_name,_var) Prop<TYPE_PROP_##_name > _var(_gc.Set(tedge()),PROP_##_name)"
  print "#define BPROP(_gc,_name,_var) Prop<TYPE_PROP_##_name > _var(_gc.Set(tbrin()),PROP_##_name)"
  print "#define GPROPX(_gc,_name,_var,_val) Prop1<TYPE_PROP_##_name > _var(_gc.Set(),PROP_##_name,_val)"
  print "#define VPROPX(_gc,_name,_var,_val) Prop<TYPE_PROP_##_name > _var(_gc.Set(tvertex()),PROP_##_name,_val)"
  print "#define EPROPX(_gc,_name,_var,_val) Prop<TYPE_PROP_##_name > _var(_gc.Set(tedge()),PROP_##_name,_val)"
  print "#define BPROPX(_gc,_name,_var,_val) Prop<TYPE_PROP_##_name > _var(_gc.Set(tbrin()),PROP_##_name,_val)"
}
