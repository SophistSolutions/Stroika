# Stroika::[Frameworks](../../)::[Auth](../)::OAuth

This Folder contains the [Frameworks](../../)::[Auth](../)::OAuth source code.

This code provides C++ support for leveraging OAuth services such as Google, Microsoft, Facebook, Twitter, etc to allow
applications to authenticate themselves.

## References

  - (https://developers.google.com/identity/protocols/oauth2/web-server#httprest_1)[https://developers.google.com/identity/protocols/oauth2/web-server#httprest_1]

## Approach

This is an early draft of OAuth support. Subject to change. I've focused on the following flow:

- Client UI (HTML) does fetch of authorization_code
  - https://accounts.google.com/o/oauth2/v2/auth?scope=openid+email&access_type=offline&response_type=code&state=xxx&redirect_uri=https%3A//oauth2.example.com/code&client_id=client_id
  - note as of 2025-01-30 - PKCE not supported (really - for apps without client secret) - server side
- GUI client asks SERVER (C++ app) to get token (where the Stroika code comes in here)
- With this token - we can easily fetch UserInfo in a number of ways

## Modules

