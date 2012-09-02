<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" omit-xml-declaration = "yes"
  encoding="iso-8859-1"/>

<xsl:variable name="bstuff">&amp;sid=</xsl:variable>

<xsl:template match="tr[count(*) = 5]">
<forum>
  <title>
    <xsl:value-of select="td[2]/span[1]/a"/>
  </title>
  <date>
    <xsl:value-of select="td[5]/span/text()"/>
  </date>
  <author>
    <xsl:value-of select="td[5]/span/a[1]/text()"/>
  </author>
  <link>
    <xsl:text>/forum/fr/</xsl:text>
    <xsl:if test=
        "string-length(substring-before(td[2]/span[1]/a/@href, $bstuff)) = 0">
      <xsl:value-of select="td[2]/span[1]/a/@href"/>
    </xsl:if>
    <xsl:if test=
        "string-length(substring-before(td[2]/span[1]/a/@href, $bstuff)) != 0">
      <xsl:value-of select="substring-before(td[2]/span[1]/a/@href, $bstuff)"/>
    </xsl:if>
  </link>
</forum>
</xsl:template>

<xsl:template match="/">
  <forums>
    <xsl:apply-templates/>
  </forums>
</xsl:template>

<xsl:template match="text()">
</xsl:template>

</xsl:stylesheet>
