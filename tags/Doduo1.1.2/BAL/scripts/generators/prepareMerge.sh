#!/bin/sh

webkit_trunk=http://svn.webkit.org/repository/webkit/trunk/
alias my_log='logger -s -t `basename $0`'

if ! [ -d $1 ] || [ -z $1 ]
then
    my_log "usage: $0 /path/to/owb_sources"
    exit 1
else
    owb_sources=${1%/} #remove last / character
    working_directory=`dirname $1`/merge
fi

# Clean and ensure that everything is OK before merging
rm -fr $working_directory
mkdir -p $working_directory/{JavaScriptCore,WebCore,WebKit,WebKitTools}
svn revert -R $owb_sources
svn update $owb_sources

# Retrieve last merge revision
old_revision=`svn log | sed -n -e '/^merge with webkit revision/p' | awk '{print $NF}' | grep -m 1 [[:digit:]]`
my_log "Last merge was at revision $old_revision"

# Create huge patch to merge
merge_revision=`svn info http://svn.webkit.org/repository/webkit/trunk/ | grep ^Revision | awk '{print $NF}'`
webkit_patch=$working_directory/webkit_merge_to_$merge_revision
my_log "Create patch between revsion $old_revision and revision $merge_revision"
for directory in {JavaScriptCore,WebCore,WebKit,WebKitTools}
do
    # This method remove directory name inside the patch
    # so we have to add it after to be able to use svn-apply script
    svn diff -r$old_revision $webkit_trunk/$directory > $webkit_patch-$directory.patch
    sed -e "s/^Index: /Index: $directory\//g" -e "s/^--- /--- $directory\//g" -e "s/^+++ /+++ $directory\//g" -i $webkit_patch-$directory.patch
    cat $webkit_patch-$directory.patch >> $webkit_patch.patch
done

# Retrieve original file for JavaScriptCore

# Retrieve original file for WebCore
for file in `cat $owb_sources/BAL/scripts/generators/data/WebKitFiles.txt`
do
    my_log "process $file"
    mkdir -p $working_directory/`dirname $file`
    svn export -r$old_revision --quiet $webkit_trunk/$file $working_directory/$file
    # Create patch for every file
    diff -Nau $working_directory/$file $owb_sources/$file > $working_directory/$file.patch
    protect_slash_owb="$(echo $owb_sources | /bin/sed -e 's/\//\\\//g')"
    sed -e "s/$protect_slash_owb\///g" -i $working_directory/$file.patch
    protect_slash_working_dir="$(echo $working_directory | /bin/sed -e 's/\//\\\//g')"
    sed -e "s/$protect_slash_working_dir\///g" -i $working_directory/$file.patch
    # Restore file in a clean state (i.e. no patch apply)
    cp $working_directory/$file $owb_sources/$file
done

# Apply patch for merge
my_log "patch $owb_sources with $webkit_patch.patch"
cd $owb_sources
WebKitTools/Scripts/svn-apply $webkit_patch.patch

# Apply patch for BAL modifications
for file in `cat $owb_sources/BAL/scripts/generators/data/WebKitFiles.txt`
do
    my_log "patch $file"
    patch -Np0 -i $working_directory/$file.patch
    if [ $? -ne 0 ]
    then
        my_log "Patch $working_directory/$file.patch fails. You MUST fix this manually in $owb_sources/$file"
    fi
done

