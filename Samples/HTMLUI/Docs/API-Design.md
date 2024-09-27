# Ask HealthFrame API Design

## Top Level Objects/Resources

### Subject

This is a person, which a health record (HR) apply to. These are lightweight IDs. That MIGHT be long-term. Or they could be short term (transient) - depending on use case.

Basic Verbs:
 - CRUD

 Example:
 - Lewis Pringle

 Example:
 - Lewis Pringle, SSN: 123-456-434, Address 349 Main Street.

---

### HR

A health record (HR) is a data object about a subject, which will be queried via a session. An HR could be
created via an uploaded HealthFrame PHR, CCR, or FHIR record collection. 

HealthRecords come in two varieties:
  - static
  - dynamic

#### Static HR

A static healthrecord is created by posting data, and this act creates an ID (cryptographic digest of the data).
So attempts to re-publish a health record object produce the old ID. These forms of health records are therefore immutable.

#### Virtual HR

A virtual HR (VHR) is a collection of merged health records. In the simplest case, a VHR might refer to
a single HR(uploaded record). Or it might refer to some policy based amalgam of records (for example, a single 
uploaded PHR, combined with the 'latest' data from the users health portal, and from their apple watch).

A VHR is a virtual summary. Its data is a policy for how to fetch / combine records. Its 'Current' property is a
readonly view of the current virtual record.

Like all HRs, a VHR refers to a single subject (so this is a contraint on the application/definition of VHR rules).

#### HR Properties

- Current
  
  This is a readonly property - the virtual summary record (in whatever format you prefer, typically FHIR). or
  for a static HR, its exactly the data that was posted.

  - Caveat - even if the record was posted in one format, it can be queried in another (allowing conversion from CCR to FHIR, for example).

- Rules (only applies to VHR)
  
  tbd, but example rules might be:
    -   HR=534508495
    -   AllHRsWithSubject=904835435
    -   HR=3, HR=5435, MostRecentHRFromAppleWatchWithTheseConnectionCredentials=xxx

  Rules may be updated.

  VHRs may refer to other VHRs, but references may not be circular.

- Subject

  This is a modifiable property associated with a health record, and indicates who the health record is 'about'.
  (INITIALLY NOT SUPPORTED, BUT EVENTUALLY MAYBE REQUIRED when uploading/constructing a health record).

---

### Session

This refers to a connection to a subject, and a series of questions or operations with respect to that subject.
These tend to be short lived, but could be persisted depending on use case, and picked up later.

A session must always refer to a single HR, and therefore a single Subject.

Basic Verbs:
 - CRUD

This is how you CHAT. (details/connection to vectordb/chat APIs TBD). For now, just can call GETPROMPT.

When you create a Session, you must specify a HR to reference.

#### Properties
 - Options
   - Auto-Apply-Changes-To-VHR: boolean

     NOTE - this requires a bit of thought. Promt generation hard todo if this is false. Chat context complicated if this is true.

   - Keep-Alive: DURATION

   - Health-Record: HR-ID

---

### User

A user is a person with an account on the system. Little thought has gone into the connection
between users and Subjects (and related security issues). for now, we simply track the 'User'
who created each object in the system. Later access-control may be appropriate.

---

## Overall Flow Example

 - Create Subject "Jim" (optional)
 - Create HR (upload FHIR record) about Subject
 - Create VHR (rule HR = created HRID - optional step)
 - Create Session (referring to some created HR)
   ask questions of the chatbot
 - Update HR (adding a second HR possibly - assuming you used Virtual HR)
 - ask more questions of the session, and the data is present
   (((questions about this flow??? maybe chat session must be based on VHR/Current - time snapshot)))



## Simple WS Example

### Upload HR, and generate prompt

~~~
  curl http://localhost:9080/HR -X POST -H "Content-Type: application/x-ccr" -d@test.ccr
~~~

Suppose returns HRID = 'd1be5539-5baf-d328-a19e-20e259c234d7'

~~~
  curl http://localhost:9080/session -X POST -H "Content-Type: application/json" -d '{"HR" : "d1be5539-5baf-d328-a19e-20e259c234d7"}'
~~~

Suppose returns session id = '093f582d-377d-71d9-32ed-c6230751b512'

~~~
  curl http://localhost:9080/session/093f582d-377d-71d9-32ed-c6230751b512/prompt
~~~
