<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" omit-xml-declaration = "yes"
  encoding="iso-8859-1"/>

<xsl:template match="@*" mode="links">
<xsl:choose>
  <xsl:when test="name() = 'href'">
     <xsl:attribute name="href">
       <xsl:text>http://delcamp.net/forum/fr/</xsl:text>
       <xsl:value-of select="string()"/>
     </xsl:attribute>
  </xsl:when>
  <xsl:when test="name() = 'src'">
     <xsl:attribute name="src">
       <xsl:text>http://delcamp.net/forum/fr/</xsl:text>
       <xsl:value-of select="string()"/>
     </xsl:attribute>
  </xsl:when>
  <xsl:otherwise>
    <xsl:copy/>
  </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="node()" mode="links">
<xsl:copy>
  <xsl:apply-templates mode="links" select="node()|@*"/>
</xsl:copy>
</xsl:template>

<xsl:template match="text()" mode="links">
<xsl:copy/>
</xsl:template>

<xsl:template match="tr[count(*) = 2 and
    ((td[1]/@class='row1' and td[2]/@class='row1'
      and td[1]/span[1]/@class='name') or
     (td[1]/@class='row2' and td[2]/@class='row2'
      and td[1]/span[1]/@class='name'))]">
  <message>
    <author>
      <xsl:value-of select="td[1]/span[1]"/>
    </author>
    <date>
      <xsl:value-of select="substring-after(
          td[2]/table/tr[1]/td[1]/span[1]/text(),': ')"/>
    </date>
    <body>
      <xsl:apply-templates mode="links" select="td[2]"/>
    </body>
    <link>
       <xsl:text>/forum/fr/</xsl:text>
       <xsl:value-of select="td[2]/table/tr[1]/td[1]/a/@href"/>
    </link>
  </message>
</xsl:template>

<xsl:template match="/">
  <messages>
    <xsl:apply-templates/>
  </messages>
</xsl:template>

<xsl:template match="text()">
</xsl:template>

</xsl:stylesheet>
