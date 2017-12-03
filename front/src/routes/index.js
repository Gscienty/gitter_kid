import React from 'react';
import BasePage from '../components/BasePage'
import { connect } from 'react-redux';
class Page extends BasePage {
    page() {
        return <div></div>;
    }
}

export default connect()(Page);