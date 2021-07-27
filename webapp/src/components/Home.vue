<template>
  <div>
    <nav
      class="navbar is-light is-fixed-top"
      role="navigation"
      aria-label="main navigation"
    >
      <div class="navbar-brand">
        <router-link :to="{ name: 'Home' }" class="navbar-item">
          <img
            src="../assets/images/logo.png"
            alt="Logo"
            style="max-width: 50px"
          />
          My App
        </router-link>
      </div>
    </nav>

    <section class="section" style="text-align: center; margin-top: 50px">
      <div class="container">
        <h1 class="title">My App</h1>

        <div class="columns is-mobile is-centered">
          <div class="column is-full">
            <img
              alt="Logo"
              src="../assets/images/logo.png"
              style="width: 100px"
            />
          </div>
        </div>

        <div class="columns">
          <div class="column">
            <button class="button is-primary" v-on:click="showStuffMessage()">
              Show stuff message
            </button>
          </div>
          <div class="column">
            <button class="button is-info" v-on:click="showCurrentTime()">
              Show current time
            </button>
          </div>
          <div class="column">
            <button class="button is-info" v-on:click="bindingReverseData()">
              Binding: Reserse data
            </button>
          </div>
          <div class="column">
            <button class="button is-info" v-on:click="bindingNetworkRequest()">
              Binding: Network request
            </button>
          </div>
          <div class="column">
            <router-link
              :to="{ name: 'About' }"
              tag="button"
              class="button is-warning"
            >
              About
            </router-link>
          </div>
        </div>

        <div
          v-if="message"
          class="columns is-mobile is-centered"
          style="margin-top: 30px; margin-bottom: 30px"
        >
          <div class="column is-full">
            <article class="message is-info">
              <div class="message-body">
                {{ message }}
              </div>
            </article>
          </div>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
/* global MyApp */

export default {
  name: "Home",
  props: {},
  data() {
    return {
      message: "",
    };
  },
  created: function () {
    // ignore
  },
  methods: {
    showStuffMessage() {
      this.message = MyApp.getStuffMessage();
    },

    showCurrentTime() {
      this.message = MyApp.getCurrentTime();
    },

    bindingReverseData() {
      let self = this;

      this.message = "Loading...";

      let payload = "my message";

      window.cefQuery({
        request: "App::ReverseData:" + payload,
        onSuccess: function (response) {
          self.message = "Response: " + response;
        },
        onFailure: function (errorCode, errorMessage) {
          self.message = "Error: " + errorCode + " / " + errorMessage;
        },
      });
    },

    bindingNetworkRequest() {
      let self = this;

      this.message = "Loading...";

      let payload = "https://httpbin.org/post";

      window.cefQuery({
        request: "App::NetworkRequest:" + payload,
        onSuccess: function (response) {
          self.message = "Response: " + response;
        },
        onFailure: function (errorCode, errorMessage) {
          self.message = "Error: " + errorCode + " / " + errorMessage;
        },
      });
    },
  },
};
</script>

<style scoped>
</style>
