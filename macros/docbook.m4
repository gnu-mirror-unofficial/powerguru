
AC_DEFUN([AC_DOCBOOK_STYLES], [
  dirlist="/usr /usr/local /opt /home/latest"
  for i in $dirlist; do
    for i in `ls -dr $i/share/sgml/docbook/xsl-stylesheets* 2>/dev/null ` ; do
       if test -f $i//html/docbook.xsl; then
         docbook_styles=`(cd $i; pwd)`
         break
       fi
    done
  done
  AC_SUBST(docbook_styles)
])

