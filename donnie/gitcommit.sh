
DIRNAME="donnie"
GITDIR="/home/lsa/marques/git_ws/lsa/assistive-robot-prog/"
DEST=$GITDIR"driver/"
BINDIR="client"

if [ -d $DEST ]; then
   #Control will enter here if $DIRECTORY exists.
  if [ ! -d $DEST$DIRNAME ]; then
	# Control will enter here if $DIRECTORY doesn't exist.
	mkdir $DEST$DIRNAME
	else 
		rm -Rf $DEST$DIRNAME/*	
  fi
  
  #if [ -d $BINDIR ]; then #if bin diretory exist
  #		rm $BINDIR/main   #remove bin files
  #fi
  
  cp -rp * $DEST$DIRNAME
  #rm $DEST$DIRNAME/gitcommit.sh
  
  #cd GITDIR
  #git add -A .
  #echo "Write a commit message: " 
  #read MSG
  #git commit -m $MSG

  else 
	echo "O diretorio destino não existe!"
fi
