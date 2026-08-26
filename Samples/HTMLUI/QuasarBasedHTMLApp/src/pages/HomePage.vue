<template>
  <q-page padding>
    <div class="row text-h4 text-center q-mb-md">
      <div class="col">Stroika HTMLUI Sample - Quasar</div>
    </div>

    <div class="justify-center row">
      <q-card class="col-11 pageCard">
        <q-card-section>
          <div class="row">
            <div class="col">
              <li> <router-link to="/about">About</router-link></li>
            </div>
          </div>
          <div class="row">
            <div class="col">
              <li> <router-link to="/authtest">AuthTest</router-link></li>
            </div>
          </div>
        </q-card-section>
      </q-card>
    </div>
  </q-page>
</template>
