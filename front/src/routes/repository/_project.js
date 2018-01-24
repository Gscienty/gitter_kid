import React from 'react';
import { Menu, Icon } from 'antd';
import BaseComponent from '../../components/BasePage'

export default class ProjectTemplate extends BaseComponent {
    unit() { }

    handleMenuOnClick(e) {

    }

    page() {
        return <div>
            <h2>{ this.props.match.params.repositoriesName } / { this.props.match.params.repositoryName }</h2>
            <Menu onClick={ this.handleMenuOnClick.bind(this) } mode="horizontal" selectedKeys={[ this.props.projectEntryUnit ]}>
                <Menu.Item key="code"><Icon type="code-o" />Code</Menu.Item>
            </Menu>
            { this.unit() }
        </div>
    }
}