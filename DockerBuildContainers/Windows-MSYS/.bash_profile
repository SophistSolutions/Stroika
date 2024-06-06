# standard thing todo for bash profiles so they include inits you would use for non-interactive shells
# if [ -f ~/.bashrc ]; then . ~/.bashrc; fi ## ~/ doesn't work in MSYS bash for some reason (// not well recieved)

# Without this we get tzset: can't find matching POSIX timezone for Windows timezone "" starting container
export TZ=America/New_York

if [ -f /.bashrc ]; then . /.bashrc; fi

echo "See Getting-Started-With-Stroika.md"
