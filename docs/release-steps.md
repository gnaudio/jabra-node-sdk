# Release Steps

This is an internal to-do list with the necessary steps for creating new releases to this repository.

## Before releasing

1. Double-check that the Azure pipeline passes. This ensures that:
    - The node SDK builds on all the three platforms (Linux, Mac, Windows)
    - `npm audit` reports no vulnerabilities
3. If release contains new events, they must be added to `electronrendererhelper`

## Beta release

1. Update version number in `package.json`. Postfix with dash beta, eg. `3.1.0-beta.3`
2. Update [Changelog](CHANGELOG.md) with beta changes
4. Publish to npm with beta tag: `npm publish --tag beta`
5. Update apps dependent on the nodesdk with the new release
- electronhelper (and publish this package to npm if needed)
- testapp
- demoapp
6. Update Jira tasks and email stakeholders

## Prod release

Follow same steps as beta release but remove `-beta.x` from version name, and remove `--tag beta` when publishing to npm. Beta sections in changelog should be merged under the new production version number.
