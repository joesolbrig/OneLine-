#An effort to create Qt helper for CatItem object
#It's a bare skeleton... 
def qdump__CatItem(d, item):
    def hashDataFirstNode(value):
        value = value.cast(hashDataType)
        bucket = value["buckets"]
        e = value.cast(hashNodeType)
        for n in xrange(value["numBuckets"] - 1, -1, -1):
            n = n - 1
            if n < 0:
                break
            if bucket.dereference() != e:
                return bucket.dereference()
            bucket = bucket + 1
        return e;

    def hashDataNextNode(node):
        next = node["next"]
        if next["next"]:
            return next
        d = node.cast(hashDataType.pointer()).dereference()
        numBuckets = d["numBuckets"]
        start = (node["h"] % numBuckets) + 1
        bucket = d["buckets"] + start
        for n in xrange(numBuckets - start):
            if bucket.dereference() != next:
                return bucket.dereference()
            bucket += 1
        return node

    keyType = item.value.type.template_argument(0)
    valueType = item.value.type.template_argument(1)

    d_ptr = item.value["d"]
    e_ptr = item.value["e"]
    size = d_ptr["size"]

    hashDataType = d_ptr.type
    hashNodeType = e_ptr.type

    check(0 <= size and size <= 100 * 1000 * 1000)
    checkRef(d_ptr["ref"])

    d.putItemCount(size)
    d.putNumChild(size)
    if d.isExpanded(item):
        isSimpleKey = isSimpleType(keyType)
        isSimpleValue = isSimpleType(valueType)
        node = hashDataFirstNode(item.value)

        innerType = e_ptr.dereference().type
        inner = select(isSimpleKey and isSimpleValue, valueType, innerType)
        with Children(d, [size, 1000], inner):
            for i in d.childRange():
                it = node.dereference().cast(innerType)
                with SubItem(d):
                    key = it["key"]
                    value = it["value"]
                    if isSimpleKey and isSimpleValue:
                        d.putName(key)
                        d.putItemHelper(Item(value, item.iname, i))
                        d.putType(valueType)
                    else:
                        d.putItemHelper(Item(it, item.iname, i))
                node = hashDataNextNode(node)

