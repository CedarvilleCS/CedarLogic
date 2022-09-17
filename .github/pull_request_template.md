<!-- Hi, this is a markdown comment (or, really, a HTML comment, which, is also an XML comment) as I said a comment.
You don't need to erase this to fill out your PR (pull-request) since it won't be rendered. 

Speaking of Rendering (sorry to people who are familiar with this, but many contributors are students due to the nature of the project)
Markdown is a Markup language. It is very simple, and what you see below. GitHub, like GitLab and others, has their own _flavor_ of Markdown.
You can read about all the cool stuff you can do in Markdown and the pieces special to GitHub here: https://github.github.com/gfm/

Use the comments below to fill out a useful/helpful PR.
-->

# Summary

<!-- Please fill out a summary of the changes you've made. Do not itemize line-by-line, the diff does that for you. Focus on intent, value, purpose, that kind of thing. -->

<!-- Please note any issues in GitHub which this relates to if applicable. You can paste the whole link, or just say `#<issue-number>` and GitHub will know what you're talking about when it renders. -->

## Testing

<!-- Please tell us between this comment and the next:
  1. What you can see this change maybe breaking
  2. How you tested to make sure those things still work
  3. Where the unit tests for the added/changed code are (if applicable)
-->

<!-- Please describe how you've tested this and check the box for each task you completed as of the latest commit. In a future day, these tests should be automated in the large-part,
but many UI related changes may still require the manual testing asked about above manual testing. -->

- [ ] Still compiles on Windows <!-- Oh! that right there is a checkbox, put an 'X' in to check it like so: [X] --> 
- [ ] Still compiles on Linux
- [ ] Unit tests run and pass

<!-- If you don't like do this stuff manually (and you shouldn't) please contribute GitHub pipelines using their matrix thing to do the basic compile test automagically. -->

## Kind of Change

Check ONE:

- [ ] Major (break in backwards compatibility or significant change in existing behavior)
- [ ] Minor (Addition of feature, (purely additive, no change in what in any significant way at a user-facing level))
- [ ] Patch (Bug fixes, refactoring (which by definition doesn't change functionality), and the like. Non-additive.)

Check if true:

- [ ] I have incremented the appropriate version number in the CMakeLists.txt file (see [Semantic Versioning](https://semver.org/)).
