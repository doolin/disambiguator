# Patent Inventor Disambiguation

**NOTE: this is a legacy code base which I've assumed development on.
I'm working to bring the code base under full test coverage, and
adding extensibility via a callback architecture.**

This project is part of an ongoing effort to match inventors
with patents, information which is useful for public policy
considerations.

The United States Patent and Trademark Office issues unique ID 
numbers for patents, but does not uniquely identify inventors.

## Contributing

Easy, fork it and go. You will need the following to compile:

* gcc4.7 or 4.8 toolchain, or a recent icc or clang release
implementing most of c++11.
* IBM's cplex package, free for academic use. You can compile
and link without cplex, but you won't be able to generate the
lookup tables. 

Don't forget to set the upstream repo for
rebasing correctly:

* `git remote add upstream git://github.com/doolin/disambiguator.git`

Before you pull from upstream or issue pull request, rebase:

* git pull --rebase upstream master

This will keep a linear commit history, which helps
debugging tremendously.

## Documentation 

The main manual is located here: [`doc/manual.pdf`](doc/manual.pdf).


## Handy links

* [Harvard Dataverse Network(DVN)](http://dvn.iq.harvard.edu/dvn/dv/patent)
* [USPTO](http://www.uspto.gov/)
* [Google Patent information distribution](http://www.google.com/googlebooks/uspto-patents.html)
