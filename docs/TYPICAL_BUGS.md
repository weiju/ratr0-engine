# Errors I made during development

  * array out of bounds errors -> crashes
  * off by one errors
  * OwnBlitter() ommitted -> weird artefacts
  * assuming that variables are automatically initialized by compiler -> weird effects
  * not taking the modulo when using circular arrays
  * copy-paste bugs
  * accidentally hardcoding parameters

# Conclusions

  * don't treat the OS as your enemy, it's your friend, use the functionalty
    that makes your life easier
  * don't forget that we are running in a multitasking environment
  * don't forget that C-compilers are not required to initialize data
  * don't assume that compilers are bug-free, but don't assume that the
    compiler is the problem before making sure that the problem isn't your code
