# standard thing todo for bash profiles so they include inits you would use for non-interactive shells
if [ -f ~/.bashrc ]; then . ~/.bashrc; elif [ -f ~/.profile ]; then . ~/.profile; fi

echo "See Getting-Started-With-Stroika.md"
