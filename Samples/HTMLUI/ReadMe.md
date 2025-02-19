# HTML UI Big Integrated Sample

- C++ Stroika-based Backend
- Modern HTML UI frontend
- optional installers
- optionally run app in docker container

This sample application shows how to build a Stroika/C++ based backend, and a simple web UI, and pack them together
into a seemless application.

This example does not show the shortest/simplest path, but is organized in a pattern I've used successfully many times to separate implementation of webservice from the RESTUL API mapping. For the shortest and most direct approach, see the [WebServer](../WebServer/) sample and directly use routes.

This sample guides you to breakup your web-service application into serveral parts:

## Backend/C++/Stroika

- [AppConfiguration.h](Backend/Sources/AppConfiguration.h) / [AppConfiguration.cpp](Backend/Sources/AppConfiguration.cpp) - automatically managed serialization of per-app-instance configuration data
- [Model.h](Backend/Sources/Model.h) / [Model.cpp](Backend/Sources/Model.cpp) - the objects you read/write/manipulate through your web service API (things that have to be serialized/deserialized)
- [IWSAPI.h](Backend/Sources/IWSAPI.h) - the abstract C++ API defininng what methods can be called (pure C++ objects, no marshalling etc)
- [WSImpl.cpp](Backend/Sources/WSImpl.cpp) - the pure application logic part of your webservices. Here you inherit from IWSAPI, and simple return the appropriate C++ object results.
- [WebServer.cpp](Backend/Sources/WebServer.cpp) - this ties together the abstract interface with URL rules (routes etc), and simply maps delegated route handlers to the IWSAPI
- [Server.cpp](Backend/Sources/Server.cpp) - register app as operating system 'service' and manage startup of components
- [Main.cpp](Backend/Sources/Main.cpp) - Command-line processing, and basic overall app startup/flow.

- To test this example:
  - Run the service (under the debugger if you wish)
  - `curl  http://localhost:80/api` 
  

## OAuth

### Tested With
- Google OAuth Provider (only so far, but setup to support others)
- non-public client (because google appears to require client secret for many apis to work)
- Supports automatic refresh token processing
- Supports revocation on logout
- As of 2025-02-19

### Reference Docs

- https://developers.google.com/identity/protocols/oauth2/web-server#httprest

### You will need your own 'CLIENT SECRET and CLIENT ID - from your OAUTH providers'

- for example from google - https://console.cloud.google.com/auth/clients?highlightClient=57741917417-6jci8aese97ppaakqniqf8e1vidn0u7q.apps.googleusercontent.com&inv=1&invt=Abot4g&project=stroika-library

### Authentication Code Flow (with PKCE)

- GET in a web browser

~~~
https://accounts.google.com/o/oauth2/v2/auth?scope=openid+profile+email&access_type=offline&include_granted_scopes=true&response_type=code&state={}&redirect_uri=http%3A//localhost:9000/oauth/google&client_id=291846620235-b7737mjsce5k6trrik7oi9b4dgum0sgg.apps.googleusercontent.com
~~~

- SHOULD PRODUCE A REDRIRECT TO (ROUGHLY): - careful site not running cuz it will capture and redirect

~~~
http://localhost:9000/oauth/google?state=%7B%7D&code=4%2F0ASVgi3LHdCv1pVgeHGlUPbFSzUKm4W3oNTefhFF8cz_vmq167UACqsuZg3gS9Rg5WLIO9g&scope=email+profile+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.profile+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+openid&authuser=0&prompt=consent
~~~

- Step 5: Exchange authorization code for refresh and access tokens

(note when extracging auth code from url, be sure to url-decode the code first since it will be re-encoded)

~~~
curl -v -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "code=4%2F0ASVgi3LHdCv1pVgeHGlUPbFSzUKm4W3oNTefhFF8cz_vmq167UACqsuZg3gS9Rg5WLIO9g&client_id=291846620235-b7737mjsce5k6trrik7oi9b4dgum0sgg.apps.googleusercontent.com&client_secret=PUT_SECRET_HERE&redirect_uri=http%3A//localhost:9000/oauth/google&grant_type=authorization_code" https://oauth2.googleapis.com/token
~~~

produces
~~~
{
    "access_token": "ya29.a0AXeO80TPyTbnL5Bpfwi6vUdnGfb1PCvcpTPTStslj4V58lbSZw-nNaLt023vW4vUlfWUvcTJevq00pFVwShJQGX2UQF5NNFx2Aj2uj5apWmHgmZgHzkthwgK6xVCZoLPg8hxIT69-IKkZRrEVY9v89-5AxWk5nF64LaFwkp4aCgYKAX0SARESFQHGX2MiqMNUCpdt6CorFTRiUtrfwQ0175",
    "expires_in": 3599,
    "refresh_token": "1//01gT2ZfM_qoW2CgYIARAAGAESNwF-L9Ir6JFWrzqwULKw0Fk9quVLg4uu-Dq-mV8TMMU_eezxIn4zGChWasvFhuWLml_kycK0xXQ",
    "scope": "openid https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/userinfo.profile",
    "token_type": "Bearer",
    "id_token": "eyJhbGciOiJSUzI1NiIsImtpZCI6ImZhMDcyZjc1Nzg0NjQyNjE1MDg3YzcxODJjMTAxMzQxZTE4ZjdhM2EiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJodHRwczovL2FjY291bnRzLmdvb2dsZS5jb20iLCJhenAiOiIyOTE4NDY2MjAyMzUtYjc3MzdtanNjZTVrNnRycmlrN29pOWI0ZGd1bTBzZ2cuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJhdWQiOiIyOTE4NDY2MjAyMzUtYjc3MzdtanNjZTVrNnRycmlrN29pOWI0ZGd1bTBzZ2cuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJzdWIiOiIxMDc1NTM5NzE3OTExMzkyNDgwMzIiLCJlbWFpbCI6Imxld2lzcHJpbmdsZUBnbWFpbC5jb20iLCJlbWFpbF92ZXJpZmllZCI6dHJ1ZSwiYXRfaGFzaCI6IjJGMHhVR2dxQV95VGtvYzVCeVRubWciLCJuYW1lIjoiTGV3aXMgUHJpbmdsZSIsInBpY3R1cmUiOiJodHRwczovL2xoMy5nb29nbGV1c2VyY29udGVudC5jb20vYS9BQ2c4b2NKdVJhN1V6LXYzdUtPZVQ3NmtKRHpHUHJEUGJEbUNVS3JtTl90cW1oVXYyMzl3M2xrVD1zOTYtYyIsImdpdmVuX25hbWUiOiJMZXdpcyIsImZhbWlseV9uYW1lIjoiUHJpbmdsZSIsImlhdCI6MTczODA2MjQwNCwiZXhwIjoxNzM4MDY2MDA0fQ.XshoVFebwxpR7wmk51ljd-eTmYMuduKgazTOSPXQe0lEn-YHHR9P5eL2kqOXqHOznpIgc1XtftfTmFsw59ipZOdQaSFBN_B1pzgu-mCbuyPf3q_fX88yMb7KVm_FlLVXqeyvn7bH7fGncgtdq5kATShfJHoVeyCnuqIQcq4tOFT6hud7lSBmBpWNJgYP6gABMWdHWuIRTYWh1FvSNVsCq1xb2vkY1CiaK_8gPXv0UkFCrmHpVogKbphrXEsKJ78Zt0OG_qjHxh4ata9qFGQBoLZthA3sjEgmx5mUVH_L4PVPSnqQ6Ib9KsZzsNoPRIPvvnq9Bi4vIID2RXUYS8rGQg"
  }
~~~

then to get the 'user_info'

~~~
curl -v -H "Authorization: Bearer ya29..." https://www.googleapis.com/oauth2/v3/userinfo
~~~
   
## Web UI

Simple app built using vue/quasar (has nothing todo with Stroika, can use any framework, but need something for a demo).
